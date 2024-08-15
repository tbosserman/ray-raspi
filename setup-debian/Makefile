all: skeleton.tar.gz setup.sh setup-auto.sh

skeleton.tar.gz:
	tar -C skel -czf skeleton.tar.gz .

setup.sh: setup_template.sh skeleton.tar.gz
	./make_setup.sh setup

setup-auto.sh: setup-auto_template.sh skeleton.tar.gz
	./make_setup.sh setup-auto
