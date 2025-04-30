#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream> // std::ofstream writer
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string_view> split(std::string_view str,
                                    std::string_view delim) {
  std::vector<std::string_view> out;

  std::string_view::size_type start = 0;
  std::string_view::size_type end = str.find(delim);

  while (end != std::string_view::npos) {
    out.push_back(str.substr(start, end - start));

    start = end + delim.size();
    end = str.find(delim, start);
  }

  out.push_back(str.substr(start, end - start));

  return out;
}

std::string_view trim(std::string_view str) {
  auto isspace = [](auto c) { return std::isspace(c); };

  auto start = std::find_if_not(str.begin(), str.end(), isspace);
  auto end = std::find_if_not(str.rbegin(), str.rend(), isspace).base();

  return std::string_view{start, std::string_view::size_type(end - start)};
}

std::string toLowerCase(std::string_view str) {
  std::string lowered;

  for (char c : str) {
    lowered.push_back(std::tolower(c));
  }

  return lowered;
}

// Allows us to use "*" in metaprogram to indicate a smart pointer
std::string fix_pointer(std::string_view field) {
  std::ostringstream out;
  std::string_view type = split(field, " ")[0];
  std::string_view name = split(field, " ")[1];
  bool close_bracket = false;

  if (type.substr(0, 12) == "std::vector<") {
    out << "std::vector<";
    type = type.substr(12, type.length() - 13); // ignore closing '>'
    close_bracket = true;
  }

  if (type.back() == '*') {
    type.remove_suffix(1);
    out << "std::shared_ptr<" << type << ">";
  } else {
    out << type;
  }

  if (close_bracket)
    out << ">";
  out << " " << name;

  return out.str();
}

void defineType(std::ofstream &writer, std::string_view baseName,
                std::string_view className, std::string_view fieldList) {

  writer << "struct " << className << " : " << baseName
         << ", public std::enable_shared_from_this<" << className << "> {\n";

  // Constructor
  writer << "  " << className << "(";

  std::vector<std::string_view> fields = split(fieldList, ", ");

  writer << fix_pointer(fields[0]);
  for (int i = 1; i < fields.size(); i++) {
    writer << ", " << fix_pointer(fields[i]);
  }

  writer << ")\n" << "      : ";

  // Store parameters in fields
  std::string_view name = split(fields[0], " ")[1];

  writer << name << "{std::move(" << name << ")}";
  for (int i = 1; i < fields.size(); i++) {
    name = split(fields[i], " ")[1];
    writer << ", " << name << "{std::move(" << name << ")}";
  }

  writer << " {}\n";

  // Fields
  writer << "\n";
  for (std::string_view field : fields) {
    writer << "  const " << fix_pointer(field) << ";\n";
  }
  writer << "};\n\n";
}

void defineAst(const std::string &outputDir, const std::string &baseName,
               const std::vector<std::string_view> &types) {
  std::string path = outputDir + "/" + baseName + ".h";
  std::ofstream writer{path};

  if (!writer) {
    std::cerr << "Error: Could not open the file for writing: " << path
              << std::endl;
  }

  writer << "#pragma once\n"
            "\n"
            "#include \"Token.h\"\n"
            "#include <any>\n"
            "#include <memory>\n"
            "#include <utility> // std::move\n"
            "#include <vector>\n"
            "\n";

  // Forward declarations for AST classes (since they reference each other)
  for (std::string_view type : types) {
    std::string_view className = trim(split(type, ": ")[0]);
    writer << "struct " << className << ";\n";
  }
  writer << "\n";

  // Base class
  writer << "struct " << baseName
         << " {\n"
            "};\n\n";

  // The AST classes
  for (std::string_view type : types) {
    std::string_view className = trim(split(type, ":")[0]);
    // space is added to the delimiter to allow for `std::any` in fields
    std::string_view fields = trim(split(type, ": ")[1]);
    defineType(writer, baseName, className, fields);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: generate_ast <output_directory>" << std::endl;
    std::exit(64);
  }

  std::string outputDir = argv[1];

  defineAst(outputDir, "Expr",
            {"Binary   : Expr* left, Token op, Expr* right",
             "Grouping : Expr* expression", "Literal  : std::any value",
             "Unary    : Token op, Expr* right"});
}
