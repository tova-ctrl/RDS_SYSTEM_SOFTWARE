cd \~/FCC\_App

rm -rf build \&\& mkdir build \&\& cd build

cmake ..

make -j$(nproc)



\#PC BEFORE DO
route add 192.168.55.1 mask 255.255.255.255 0.0.0.0 if 17 metric 1





**# On Jetson**

**cd \~/FCC\_App/build**

**make**

**./fcc\_app eth:192.168.55.3 192.168.55.2 5200**



**cd FCC\_App/build          # ספריית הבנייה**

**make -j$(nproc)           # בונה מחדש את fcc\_app עם השינוי**



**sudo systemctl restart fcc\_app   # אפשר מכל מקום אחרי זה**



