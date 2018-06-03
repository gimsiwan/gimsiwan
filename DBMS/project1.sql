select name from Trainer where hometown = 'Blue City';
select name from Trainer where hometown = 'Brown City' or hometown = 'Rainbow City';
select name, hometown from Trainer where name like 'a%' or name like 'e%' or name like 'o%' or name like 'u%' or name like 'i%';
select name from Pokemon where type='water';
select distinct type from Pokemon;
select name from Pokemon order by name;
select name from Pokemon where name like '%s';
select name from Pokemon where name like '%e%s';
select name from Pokemon where name like 'a%' or name like 'e%' or name like 'o%' or name like 'u%' or name like 'i%';
select type, count(name) as number from Pokemon group by type;
select nickname from CatchedPokemon order by level desc limit 3;
select avg(level) as averageLevel from CatchedPokemon;
select max(level)-min(level) as gap from CatchedPokemon;
select count(name) as number from Pokemon where name between 'b' and 'f' and name not like 'f%';
select count(name) as number from Pokemon where type not in ('fire','grass','water','electric');
select Trainer.name as trainer, Pokemon.name as Pokemon, nickname from Trainer, Pokemon, CatchedPokemon where CatchedPokemon.pid=Pokemon.id and CatchedPokemon.owner_id=Trainer.id and CatchedPokemon.nickname like '% %';
select distinct Trainer.name as trainer from Trainer, CatchedPokemon, Pokemon where CatchedPokemon.pid=Pokemon.id and Pokemon.type = 'Psychic' and Trainer.id = CatchedPokemon.owner_id;
select Trainer.name as trainer, Trainer.hometown as hometown from Trainer, CatchedPokemon where CatchedPokemon.owner_id=Trainer.id group by owner_id order by avg(level) desc limit 3;
select Trainer.name as trainer, count(pid) as catchNumber from Trainer, CatchedPokemon where CatchedPokemon.owner_id=Trainer.id group by owner_id order by count(pid) desc, Trainer.name desc;
select Pokemon.name as name, CatchedPokemon.level as level from Pokemon, CatchedPokemon, Gym where Gym.city='Sangnok City' and Gym.leader_id = CatchedPokemon.owner_id and CatchedPokemon.pid=Pokemon.id order by CatchedPokemon.level;
select Pokemon.name, count(CatchedPokemon.pid) as count from Pokemon left join CatchedPokemon on Pokemon.id = CatchedPokemon.pid group by Pokemon.id order by count(CatchedPokemon.pid) desc;
select name from Pokemon where id = (select after_id from Evolution where before_id = (select after_id from Evolution, Pokemon where before_id = id and name = 'Charmander'));
select distinct Pokemon.name from CatchedPokemon, Pokemon where Pokemon.id=CatchedPokemon.pid and Pokemon.id<=30 order by Pokemon.name;
select Trainer.name as trainer, Pokemon.type as type from Pokemon, Trainer, CatchedPokemon where CatchedPokemon.owner_id = Trainer.id  and CatchedPokemon.pid=Pokemon.id  group by CatchedPokemon.owner_id having count(distinct Pokemon.type)=1;
select Trainer.name as name, Pokemon.type as type, count(*) as count from Trainer, CatchedPokemon, Pokemon where CatchedPokemon.owner_id = Trainer.id and CatchedPokemon.pid = Pokemon.id group by Trainer.name, Pokemon.type;
select Trainer.name as trainer, Pokemon.name as pokemon,count(*) as catchNum from Trainer, CatchedPokemon, Pokemon where CatchedPokemon.owner_id = Trainer.id and CatchedPokemon.pid=Pokemon.id group by owner_id having count(distinct pid) =1;
select Trainer.name as trainer, Gym.city as city from Trainer, Gym, CatchedPokemon, Pokemon where Gym.leader_id = Trainer.id and CatchedPokemon.owner_id = Trainer.id and CatchedPokemon.pid = Pokemon.id group by leader_id having count(distinct type) >1;
select Trainer.name as name, sum(level) as levelSum from Gym left join CatchedPokemon on CatchedPokemon.level>=50 and leader_id=owner_id, Trainer where Trainer.id=leader_id group by Gym.city;
select Pokemon.name as pokemon from Pokemon, Trainer, CatchedPokemon where CatchedPokemon.pid = Pokemon.id and CatchedPokemon.owner_id = Trainer.id and (hometown = 'Sangnok City' or hometown ='Blue City') group by Pokemon.id having count(distinct hometown)>1;
select Pokemon.name as pokemon from Pokemon, Evolution where not exists (select before_id from Evolution where Pokemon.id = before_id) and id=after_id;
