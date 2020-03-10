DROP TABLE IF EXISTS Students CASCADE;
create table Students(
    zid integer primary key,
    name PersonName not null
);

insert into Students(zid,name)values
(9300035,'Shepherd, John Andrew');