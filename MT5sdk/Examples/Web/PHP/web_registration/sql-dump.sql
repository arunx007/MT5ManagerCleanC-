CREATE TABLE `activation` (
   `id` int(10) unsigned NOT NULL auto_increment,
   `create_time` datetime NOT NULL default '0000-00-00 00:00:00',
   `activation_key` TEXT,
   `activated` tinyint(1) NOT NULL default '0',
   PRIMARY KEY  (`id`)) 
ENGINE=InnoDB DEFAULT CHARSET=utf8