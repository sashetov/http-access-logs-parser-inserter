DROP DATABASE IF EXISTS statistics_entities;
CREATE DATABASE statistics_entities;
USE statistics_entities;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';

CREATE TABLE browsers (
    id              INT             NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(255)    NOT NULL,
    version         VARCHAR(255)    NOT NULL,
    UNIQUE          KEY             `name_version_uniq` (`name`,`version`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE ips (
    id              INT             NOT NULL AUTO_INCREMENT PRIMARY KEY,
    ipv4            INT UNSIGNED    NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE pages (
    id              INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(255) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE locations (
    id              INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(3) NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE domains (
  id                INT(11)       NOT NULL,
  name              VARCHAR(255)  NOT NULL,
  alias_of          INT(11)       NOT NULL,
  uid               INT(11)       NOT NULL,
  UNIQUE KEY       `id`           (`id`),
  UNIQUE KEY       `name`         (`name`),
  UNIQUE KEY       `name_a_u`     (`name`,`alias_of`,`uid`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE users (
  id           INT(11)       NOT NULL,
  username     VARCHAR(255)  NOT NULL,
  password     VARCHAR(255)  NOT NULL,
  UNIQUE KEY  `id`           (`id`),
  UNIQUE KEY  `uusername`    (`username`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE oses (
    id              INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(255) NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE referers (
    id              INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(255) NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE search_terms (
    id              INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name            VARCHAR(255) NOT NULL UNIQUE
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

/*
INSERT IGNORE INTO statistics_entities.domains(id,name,alias_of,uid) SELECT DISTINCT did,domain_name,alias_of,uid FROM cluster.domains;
INSERT IGNORE INTO statistics_entities.users(id,username,password)  SELECT DISTINCT uid,username,password FROM cluster.domains;
/
