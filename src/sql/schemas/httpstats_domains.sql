DROP DATABASE IF EXISTS httpstats_domains;
CREATE DATABASE httpstats_domains;
USE httpstats_domains;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE domains        ( id INT(11) NOT NULL,            name VARCHAR(255) NOT NULL,     alias_of   INT(11) NOT NULL,    uid   INT(11) NOT NULL, UNIQUE KEY `id`   (`id`),      UNIQUE KEY `name` (`name`), UNIQUE KEY `name_a_u`           (`name`,  `alias_of`,   `uid`   )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_hourly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL,         domains_id INT NOT NULL,        count INT NOT NULL,     KEY `date` (`date`),           UNIQUE KEY `id`   (`id`),   UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_daily     ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL,         domains_id INT NOT NULL,        count INT NOT NULL,     KEY `date` (`date`),           UNIQUE KEY `id`   (`id`),   UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_weekly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL,         domains_id INT NOT NULL,        count INT NOT NULL,     KEY `date` (`date`),           UNIQUE KEY `id`   (`id`),   UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_monthly   ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL,         domains_id INT NOT NULL,        count INT NOT NULL,     KEY `date` (`date`),           UNIQUE KEY `id`   (`id`),   UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE hits_yearly    ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL,         domains_id INT NOT NULL,        count INT NOT NULL,     KEY `date` (`date`),           UNIQUE KEY `id`   (`id`),   UNIQUE KEY `date_domains_count` ( `date`, `domains_id`, `count` )) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
