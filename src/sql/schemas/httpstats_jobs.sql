DROP DATABASE IF EXISTS httpstats_vu;
CREATE DATABASE httpstats_domains;
USE httpstats_domains;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE dstats_comp_range_hrs    ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE dstats_comp_range_days   ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE dstats_comp_range_weeks  ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE dstats_comp_range_months ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE dstats_comp_range_years  ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE dstats_comp_range_totals ( id INT NOT NULL AUTO_INCREMENT, start_ts DATETIME NOT NULL, end_ts DATETIME NOT NULL, domains_id INT NOT NULL, UNIQUE KEY `id` (`id`), UNIQUE KEY `st_et` (`start_ts`, `end_ts`, `domains_id`)) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
