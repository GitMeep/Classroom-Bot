DROP TABLE IF EXISTS "Settings" CASCADE;
DROP TABLE IF EXISTS "Hands" CASCADE;
DROP TABLE IF EXISTS "Questions" CASCADE;
DROP TABLE IF EXISTS "MutedUsers" CASCADE;
DROP TABLE IF EXISTS "MutedChannels" CASCADE;

CREATE TABLE "Settings" (
  "guildId" BIGINT NOT NULL PRIMARY KEY,
  "prefix" TEXT NOT NULL,
  "roleName" TEXT NOT NULL,
  UNIQUE("guildId"));

CREATE TABLE "Hands" (
  "guildId" BIGINT not null,
  "userId" BIGINT not null,
  "raisedWhen" timestamp without time zone not null,
  UNIQUE("guildId", "userId"));

CREATE TABLE "Questions" (
  "guildId" BIGINT NOT NULL,
  "userId" BIGINT NOT NULL,
  "question" TEXT NOT NULL,
  "askedWhen" timestamp without time zone not null);

CREATE TABLE "MutedUsers" (
  "guildId" bigint not null,
  "userId" bigint not null,
  UNIQUE("guildId", "userId"));

CREATE TABLE "MutedChannels" (
  "channelId" bigint not null primary key,
  UNIQUE("channelId"));


INSERT INTO "Settings" ("guildId", "prefix", "roleName")
SELECT guild_id, prefix, role
FROM guild_settings;
