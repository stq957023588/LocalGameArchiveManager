create table if not exists game_archive
(
    game_name text    not null,
    md5       text    not null,
    time      integer not null,
    PRIMARY KEY (game_name, time)
)