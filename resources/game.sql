create table if not exists game
(
    game_name         text PRIMARY KEY not null,
    icon              blob,
    startup_file_path text,
    archive_path      text             not null
)