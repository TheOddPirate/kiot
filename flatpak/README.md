So this was just a folder i created to play around with flatpak and learn some more about it. 

I ended up creating a mqtt update entity for kiot.
and from there my brain just got an overflow of ideas of how it 
could be possible to
Create a workflow on github that builds a flatpak bundle and uploads it to a release automatically on merge, then all we need is a update integration using the update entity and maybe it would be possible to update our own
flatpak setup of kiot from HA.

flatpak-builder --repo=./build/flatpak-repo --force-clean build-installer .flatpak-manifest.yaml

flatpak build-bundle ./build/flatpak-repo kiot.flatpak org.davidedmundson.kiot