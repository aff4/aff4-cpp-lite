if HAVE_DOXYGEN

dist_doc_DATA = html/*

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
   
   
html/*:
	$(DOXYGEN)
           
endif 
