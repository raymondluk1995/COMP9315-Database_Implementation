DROP TABLE IF EXISTS Students CASCADE;
create table Students(
    zid integer primary key,
    name PersonName not null
);

insert into Students(zid,name)values
(9300030,'Shepherd,John Andrew');

insert into Students(zid,name)values
(9300031,'Smith, John');

insert into Students(zid,name)values
(9300032,"O'Brien, Patrick Sean");

--insert into Students(zid,name)values
--(9300033,'Mahagedara Patabendige,Minosha Mitsuaki Senakasiri');

--insert into Students(zid,name)values
--(9300034,'I-Sun,Chen Wang');

--insert into Students(zid,name)values
--(9300036,'Clifton-Everest,Charles Edward');

--insert into Students(zid,name)values
--(9300037,'Smith,Harold');

create index on Students using hash (name);

select a.zid, a.name, b.zid from Students a join Students b on (a.name = b.name);


--select family(name), given(name), show(name)
--from   Students;	

select family(name) from Students;	
select given(name) from Students;
select show(name) from Students;