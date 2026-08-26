cp -r "/mnt/c/Projects/ראיונות_עבודה/אלפו_סיסטמס/FCC_App/." ~/FCC_App/FCC_App/
scp -r "C:\Projects\RDS_SYSTEM_SOFTWARE\FCC_App" aleppo@192.168.55.1:~/projects/rds_system/

scp "C:\Projects\RDS_SYSTEM_SOFTWARE\FCC_App\src\FccController.cpp" aleppo@192.168.55.1:~/projects/rds_system/FCC_App/src/FccController.cpp
scp "C:\Projects\RDS_SYSTEM_SOFTWARE\FCC_App\src\main.cpp" aleppo@192.168.55.1:~/projects/rds_system/FCC_App/src/main.cpp
scp "C:\Projects\RDS_SYSTEM_SOFTWARE\FCC_App\src\HttpServer.cpp" aleppo@192.168.55.1:~/projects/rds_system/FCC_App/src/HttpServer.cpp
scp "C:\Projects\RDS_SYSTEM_SOFTWARE\FCC_App\src\EthernetServer.cpp" aleppo@192.168.55.1:/projects/rds_system/FCC_App/src/EthernetServer.cpp
scp EthernetServer.cpp aleppo@192.168.55.1:projects/rds_system/FCC_App/src/
scp HttpServer.cpp aleppo@192.168.55.1:projects/rds_system/FCC_App/src/

cd ~/FCC_App/FCC_App && make clean
make
sudo ./fcc_app eth:192.168.55.3 192.168.55.2 5200