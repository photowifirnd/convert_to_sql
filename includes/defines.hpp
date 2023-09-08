#ifndef DEFINES_HPP
#define DEFINES_HPP
//Constants
const int MAX_ROWS = 50000;
//Header definitions for a MySql database
#define SQL_HEADER "SET SQL_MODE = \"NO_AUTO_VALUE_ON_ZERO\";\nSTART TRANSACTION;\nSET time_zone = \"+00:00\";\n\n"
//Header set Tables for MySql database
#define getCreateTable(dbName, tName) \
    "CREATE TABLE IF NOT EXISTS `" + dbName + "`.`" + tName + "` (\n"
#define setCreateColumn(columnName, columnType) \
    "  `" + columnName + "` " + columnType + " NULL,"
#define setPrimaryKey(columnName) \
    "PRIMARY KEY (`" + columnName + "`)\n"
#define MYSQL_ENGINE ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;\n"
#define openSQLInsert(dbName, tName) \
    "INSERT INTO " \
    "`" + dbName + "`.`" + tName + "` (" 
#define setColumName(columnName) \
    "`" + columnName + "`"
#define closeSQLInsert() \
 ") VALUES\n"
#define SQL_FOOTER "\n\nCOMMIT;"
//Types
#define VARCHAR "varchar(255)"
#define INT "int(11)"
#define TINYINT "tinyint(4)"
#define FLOAT "float"
#define DOUBLE "double"
#define DATE "date"
#define DATETIME "datetime"
#define TIMESTAMP "timestamp"
#define TIME "time"
#define YEAR "year"
#define TEXT "text"
#define TINYTEXT "tinytext"
#define MEDIUMTEXT "mediumtext"
#define LONGTEXT "longtext"
#define BLOB "blob"
#define TINYBLOB "tinyblob"
#define MEDIUMBLOB "mediumblob"
#define LONGBLOB "longblob"
#define ENUM "enum"
#define SET "set"
#define BIT "bit"
#define BOOL "bool"
#define BOOLEAN "boolean"
#define SERIAL "serial"
#define DECIMAL "decimal"
#define NUMERIC "numeric"
#define FIXED "fixed"
#define REAL "real"
#define DOUBLE_PRECISION "double precision"
#endif