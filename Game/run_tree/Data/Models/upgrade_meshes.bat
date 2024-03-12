@echo off
for %%f in (*.mesh) do (
	echo Upgrading mesh file [%%f]:
	meshconv upgrade %%f -o ./
)