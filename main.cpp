#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>       // std::string
#include <vector>       // std::vector
#include <array>        // std::array
#include <iterator>     // std::iterator
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "includes/defines.hpp"

std::string ft_trim_quotes(std::string &str){
  while (str.find("\"") != std::string::npos) { 
    str = str.replace(str.find("\""), 1, "");
  }
  return str;
}
size_t ft_strlen(const char *str){
  int i = 0;
  while (str[i] != '\0') {
    i++;
  }
  return i;
}
int ft_error(int error_code, std::string error_message){
  write(2, error_message.c_str(), ft_strlen(error_message.c_str()));
  exit(error_code);
}
bool isDelimiter(char c, char *delimiter) {
  if (c == 't') {
    *delimiter = '\t';
    return true;
  } else if (c == 'c') {
    *delimiter = ',';
    return true;
  }
  return false;
}
int ft_delFile(const std::string &file_name){
  if (remove(file_name.c_str()) != 0) {
    return -1;
  }
  return 0;
}
int parseHeaderLine(const std::vector<std::string> header, const std::string &outputFile, const std::string &dbName, const std::string &tName){
  std::vector<std::string> parsedHeader;
  std::vector<std::string>::const_iterator it = header.begin();
  bool first = true;
  std::ofstream fs;
  std::string primary_key;
  std::cout << "Parsing header line: " << std::endl;
  fs.open((outputFile.substr(0, outputFile.find_last_of('.')) + ".sql"), std::ofstream::app);
  if (!fs.is_open()) {
    return -1;
  }
  fs.seekp(0, std::ios_base::end);
  fs << SQL_HEADER;
  fs << getCreateTable(dbName, tName);
  while (it != header.end()) {
    std::string column = *it;
    ft_trim_quotes(column);
    if (first) {
      primary_key = setPrimaryKey(column);
      first = false;
    }
    fs << setCreateColumn(column, VARCHAR) << std::endl;
    it++;
  }
  
  fs << primary_key <<  MYSQL_ENGINE ;
  fs.close();
  return 0;
}
/**
 * @brief Split into insert of 50,000 rows that way we can avoid the max_allowed_packet error
*/
int splitInsert(std::vector<std::string> header, std::vector<std::vector<std::string>> rows, const std::string &dbName, const std::string &tName, const std::string &fName){
  std::vector<std::vector<std::string>>::iterator itRows = rows.begin();
  std::vector<std::string>::iterator itHeader = header.begin();
  std::ofstream fs;
  if (rows.size() == 0) {
    return -1;
  }
  fs.open(fName, std::ofstream::app);
  if (!fs.is_open()) {
    return -1;
  }
  fs << openSQLInsert(dbName,tName);
  while (itHeader != header.end()){
    fs << setColumName(ft_trim_quotes(*itHeader));
    if (itHeader != header.end() - 1) {
      fs << ",";
    }
    itHeader++;
  }
  fs << closeSQLInsert();
  
  while (itRows != rows.end()) {
    std::vector<std::string>::iterator it2 = itRows->begin();
    while (it2 != itRows->end()) {
      if (it2 == itRows->begin()) {
        fs << "(" << *(it2++) << ",";
      } else if (it2 == itRows->end() - 1) {
        fs << *(it2++) << ")";
      } else {
        fs << *(it2++) << ",";
      }
    }
    if (itRows == rows.end() - 1) {
      fs << ";\n";
    } else {
      fs << ",\n";
    }
    itRows++;
  }
  fs << "COMMIT;\n";
  fs.close();
  return 0;
}
int saveToSQLFile(const std::string &outputFile, const std::string &dbName, const std::string &tName, const std::vector<std::vector<std::string>> &totalRows){
  std::vector<std::vector<std::string>>::const_iterator itRows = totalRows.begin() + 1;
  std::vector<std::string> header = totalRows[0];
  std::vector<std::vector<std::string>> rows;
  std::string fName = outputFile.substr(0, outputFile.find_last_of('.')) + ".sql";
  std::cout << "Saving to SQL file: " << fName << std::endl;
  ft_delFile(fName);
  parseHeaderLine(header, outputFile, dbName, tName);
  
  if (totalRows.size() -1 <= MAX_ROWS) {
    std::cout << "Total rows <= 50.000: " << totalRows.size() - 1 << std::endl;
    rows.assign(totalRows.begin() + 1, totalRows.end());
    splitInsert(header, rows, dbName, tName, fName);
    return 0;
  } else {
    std::cout << "Total rows > 50.000: " << totalRows.size() - 1 << std::endl;
    int blocks = (totalRows.size() - 1) / MAX_ROWS;
    int remain = (totalRows.size() - 1) % MAX_ROWS;
    while (blocks >= 0) {
      std::cout << "Blocks: " << blocks << std::endl;
      rows.clear();
      if (blocks > 0){
        rows.assign(itRows, itRows + MAX_ROWS);
        itRows += MAX_ROWS;
      }
      else {
        rows.assign(itRows, itRows + remain);
      }
      splitInsert(header, rows, dbName, tName, fName);
      blocks--;
    }
  }
  int cnt = 0;
  std::cout << "Finishing process." << std::endl;
  rows.clear();
  return 0;
}
int saveToCSVFile(const std::string &output_file, std::vector<std::vector<std::string>> &columns){
  std::ofstream fs;
  std::vector<std::vector<std::string>>::iterator it = columns.begin();
  std::string fName = output_file.substr(0, output_file.find_last_of('.')) + ".out";
  std::cout << "Saving to CSV file: " << fName << std::endl;
  ft_delFile(fName);
  fs.open(fName, std::ofstream::out);
  if (!fs.is_open()) {
    return -1;
  }
  while (it != columns.end()) {
    std::vector<std::string>::iterator it2 = it->begin();
    while (it2 != it->end()) {
      if (it2 == it->end() - 1) {
        fs << *(it2++);
      } else {
        fs << *(it2++) << ",";
      }
    }
    if (it != columns.end() - 1) {
        fs << "\n";
    }
    it++;
  }
  fs.close();
  return 0;
}
std::string parseColumn(std::string &column) {
  int pos = 0;
  int found = 0;
  while ((found = column.find("\"", pos)) != std::string::npos) {
    column = column.replace(found, 1, "\\\"");
    pos = found + 2;
    found = column.find("\"", pos);
  }
  return column;
}
std::vector<std::string> parseLine(std::string &line, const char &delimiter) {
  std::vector<std::string> parsedLine;
  std::string column = "";
  size_t pos = 0;
  size_t found = 0;
  while ((found = line.find(delimiter, pos)) != std::string::npos) {
    std::string column = line.substr(pos, found - pos);
    
    column = parseColumn(column);
    parsedLine.push_back("\"" + column + "\"");
    pos = found + 1;
    found = line.find(delimiter, pos);
  }
  parsedLine.push_back("\"" + line.substr(pos, line.length() - pos) + "\"");
  return parsedLine;
}
int ft_readFile(const std::string &file_name, std::vector<std::vector<std::string>> &columns, char *delimiter){
  std::ifstream ifs;
  std::string line;
  ifs.open(file_name, std::ifstream::in);
  line.clear();
  if (!ifs.is_open()) {
    return -1;
  }
  while(std::getline(ifs,line)) {
    columns.push_back(parseLine(line, *delimiter));
  }
  ifs.close();
  return 0;
}
/// @brief 
/// @return 
int main (int argc, char *argv[]) {
  std::vector<std::vector<std::string>> columns;
  char delimiter = '\t';
  int ret = 0;
  std::cout << "Starting process, this may take a while.\nWait to finish... " << std::endl;
  if (argc != 5) {
    std::cout << "Usage: " << argv[0] << " <file_name> <databaseName> [delimiter: c as coma - t as tab]" << std::endl;
    return 1;
  }
  if (argc == 4 && !isDelimiter(argv[4][0], &delimiter)) {
    std::cout << "Usage: " << argv[0] << " <file_name> <databaseName> <tableName> [delimiter: c as coma - t as tab]" << std::endl;
    return 1;
  }
  if ((ret = ft_readFile(argv[1], columns, &delimiter))){
    ft_error(ret, "Fatal error: Couldn't read file\n");
  }
  if ((ret = saveToCSVFile(argv[1], columns))){
    ft_error(ret, "Fatal error: Couldn't save CSV to file\n");
  }
  if ((ret = saveToSQLFile(argv[1], argv[2], argv[3], columns))){
    ft_error(ret, "Fatal error: Couldn't save SQL to file\n");
  }
  return 0;
}