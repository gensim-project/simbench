== Compilation == 

ARCH=arm PLATFORM=versatile make

== Run it with qemu ==

qemu-system-arm -machine type=versatileab -kernel ./out/arm/versatile/simbench -nographic 

== Report ==

stdbuf qemu-system-arm -machine type=versatileab -kernel ./out/arm/versatile/simbench -nographic | host/host

