ALTER TABLE `test`.`login` ADD COLUMN `department` VARCHAR(50) NULL DEFAULT NULL  AFTER `banned` , ADD COLUMN `course` TEXT NULL DEFAULT NULL  AFTER `department` , ADD COLUMN `yearlevel` VARCHAR(10) NULL DEFAULT NULL  AFTER `course` ;
ALTER TABLE `test`.`login` ADD COLUMN `cardnum` VARCHAR(20) NULL  AFTER `loginid` ;
CREATE  TABLE `test`.`loginrecords` (

  `idloginrecords` INT NOT NULL AUTO_INCREMENT ,

  `cardnum` VARCHAR(15) NOT NULL ,

  `date` DATETIME NOT NULL ,

  PRIMARY KEY (`idloginrecords`) );

ALTER TABLE `test`.`login` CHANGE COLUMN `banned` `banned` INT(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 0  ;

ALTER TABLE `test`.`login` DROP COLUMN `middlename` ;
