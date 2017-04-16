DROP DATABASE IF EXISTS httpstats_browsers;
CREATE DATABASE httpstats_browsers;
USE httpstats_browsers;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';
CREATE TABLE browsers ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255) NOT NULL, version VARCHAR(255) NOT NULL,
  UNIQUE KEY `name_version_uniq` (`name`,`version`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE browsers_hourly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, browser_id INT NOT NULL, count INT NOT NULL,
  KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` )
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE browsers_daily ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, browser_id INT NOT NULL, count INT NOT NULL,
  KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` )
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE browsers_weekly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, browser_id INT NOT NULL, count INT NOT NULL,
  KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` )
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

CREATE TABLE browsers_monthly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, browser_id INT NOT NULL, count INT NOT NULL,
  KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` )
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_yearly ( id INT NOT NULL AUTO_INCREMENT, date DATETIME NOT NULL, domains_id INT NOT NULL, browser_id INT NOT NULL, count INT NOT NULL,
KEY `date` (`date`), UNIQUE KEY `id` (`id`), UNIQUE KEY `date_domains_browser_count` ( `date`, `domains_id`, `browser_id`, `count` )
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
CREATE TABLE browsers_total ( id INT NOT NULL AUTO_INCREMENT, count INT NOT NULL
  UNIQUE KEY `id` (`id`), 
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
