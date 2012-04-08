CREATE TABLE `loginsheet` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cardnum` varchar(15) NOT NULL,
  `in` datetime DEFAULT NULL,
  `out` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

