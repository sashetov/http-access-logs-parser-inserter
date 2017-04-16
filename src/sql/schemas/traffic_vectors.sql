DROP DATABASE IF EXISTS httpstats_tvectors;
CREATE DATABASE httpstats_tvectors;
USE httpstats_tvectors;
SET GLOBAL innodb_file_per_table=1;
SET GLOBAL innodb_file_format=Barracuda;
SET GLOBAL innodb_compression_level=9;
SET GLOBAL tx_isolation = 'READ-COMMITTED';

--tvector_inn ids are stored in the primary key here, so the tinn vectors are indexed here. 
CREATE TABLE tvectors_inn (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, -- tv_inner_id
  a_id INT NOT NULL, -- page_id of referring page 
  b_id INT NOT NULL, -- page_id of referred page - this page is an exit point if its the last point of all traffic
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--HOURLY inner traffic ( based on direction of pageviews as analyzed from hops by visitor with the same ip and time delta < than 30 min )
CREATE TABLE tvinn_hourly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinn_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `inner_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinn_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--DAILY inner traffic ( based on direction of pageviews as analyzed from hops by visitor with same ip and time delta < 1h )
CREATE TABLE tvinn_daily(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinn_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `inner_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinn_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--WEEKLY inner traffic ( based on direction of pageviews as analyzed from hops by visitor with the same ip and time delta < 1d )
CREATE TABLE tvinn_weekly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinn_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `inner_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinn_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--MONTHLY inner traffic ( based on direction of pageviews as analyzed from hops visitors with the same ip and with time delta < 1w )
CREATE TABLE tvinn_monthly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinn_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `inner_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinn_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--YEARLY inner traffic ( based on direction of pageviews as analyzed from hops visitors with the same ip and with time delta < 1m )
CREATE TABLE tvinn_yearly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinn_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `inner_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinn_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--TOTAL  inner traffic ( based on direction of pageviews as analyzed from hops visitors with the same ip and with time delta < 1m )
CREATE TABLE tvinn_total(
  int id        INT         NOT NULL AUTO_INCREMENT, --should match up 1:1 to tvinn_id
  int count     INT         NOT NULL
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

-- TOTAL incoming traffic vectors ( referrer links only )
CREATE TABLE tvectors_inc(
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, -- tv_incoming_id
  a_id INT NOT NULL, -- referer_id of referring external page
  b_id INT NOT NULL, -- page_id of landing page
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;

-- HOURLY incoming traffic vector ( referrer links only )
CREATE TABLE tvinc_hourly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinc_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `incoming_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinc_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
-- DAILY incoming traffic vector ( referrer links only )
CREATE TABLE tvinc_daily(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinc_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `incoming_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinc_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
-- WEEKLY incoming traffic vector ( referer links only )
CREATE TABLE tvinc_weekly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinc_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `incoming_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinc_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
-- MONTHLY incoming traffic vector ( analysis by referer links )
CREATE TABLE tvinc_monthly(
  id INT NOT NULL AUTO_INCREMENT,
  date DATETIME NOT NULL,
  domains_id INT NOT NULL,
  tvinc_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `incoming_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinc_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
-- YEARLY incoming traffic vector ( analysis by referer links )
CREATE TABLE tvinc_yearly(
  id INT NOT NULL AUTO_INCREMENT,
  int YEAR NOT null 
  domains_id INT NOT NULL,
  tvinc_id INT NOT NULL,
  UNIQUE KEY `id` (`id`),
  UNIQUE KEY `date` (`date`)
  UNIQUE KEY `incoming_traffic_id` (`start_ts`, `end_ts`, `domains_id`, `tvinc_id`)
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
--TOTAL  inner traffic ( based on direction of pageviews as analyzed from hops visitors with the same ip and with time delta < 1m )
CREATE TABLE tvinc_total(
  int id        INT         NOT NULL AUTO_INCREMENT, --should match up 1:1 to tvinn_id
  int count     INT         NOT NULL
) ROW_FORMAT=COMPRESSED KEY_BLOCK_SIZE=16;
