tests:
	set -e; \
	for i in */Makefile; do \
		echo '==== Making:' $$(dirname $$i); \
		make -C $$(dirname $$i); \
	done

runtests:
	set -e; \
	for i in */Makefile; do \
		echo '==== Running:' $$(dirname $$i); \
		$$(dirname $$i)/$$(dirname $$i).out; \
	done

clean:
	set -e; \
	for i in */Makefile; do \
		echo '==== Cleaning:' $$(dirname $$i); \
		make -C $$(dirname $$i) clean; \
	done

zonedb:
	set -e; \
	for i in $$(find -name 'zonedb*'); do \
		make -C $$i; \
	done
