@attrib -r buildnumber.txt
@attrib -r buildnumber.h
@gawk -F"," "/movff/{$NF+=1;}1" OFS=","  buildnumber.txt >buildnumber.h
@copy buildnumber.h buildnumber.txt
@attrib +r buildnumber.txt
@attrib +r buildnumber.h
