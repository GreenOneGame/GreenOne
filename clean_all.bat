@echo off

rmdir /s /q Saved

rmdir /s /q Intermediate

rmdir /s /q DerivedDataCache

rmdir /s /q Binaries

rmdir /s /q .idea

rmdir /s /q .vs

rmdir /s /q Build

del /q .vsconfig

del /q *.sln


