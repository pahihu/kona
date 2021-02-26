/ some classic APL benchmarks, from
/ 'Dyalog APL Arrives in the US' by Gregg Taylor
/ translated by isawdrones

`0: "Generating test data...\n\n"

/ in paper, these values are used:
/ vs: 1000
/ pms: 50 100

/ more realistic values for modern hardware:
vs: 10000
ms: 500 10000

cm: ms#cv: " ABCDEFGHIJ"@,/1?'vs#11
im: ms#iv: ,/1?'vs#500
fm: ms#fv: iv+0.01*1!iv
bm: ms#bv: ,/1?'vs#2

tests: (("int add\t" ; 200; "im+im+im+im+im")
        ("fp add\t" ; 200; "fm+fm+fm+fm+fm")
        ("int mult" ; 200; "im*im*im*im*im")
        ("fp mult\t" ; 200; "fm*fm*fm*fm*fm")
        ("index\t" ; 250; "(!#bv)@&bv" )
        ("char compr" ; 200; "cv@&bv" )
        ("int compr" ; 200; "iv@&bv" )
        ("int + red" ; 200; "+/iv" )
        ("int | red" ; 200; "|/iv" )
        ("bool scan" ; 100; "(~=)\\bm" )
        ("mat rotate" ; 100; "(!500)!\\:cm" )
        ("char trans" ; 100; "+cm" )
        ("int trans" ; 100; "+im" )
        ("vec of vecs"; 200; ",:'cm" )
        ("partition" ; 200; "aa:(&bv)_ cv" )
        ("shape each" ; 200; "^:'aa" )
        ("vec compar" ; 200; "&/cv=cv" )
        ("int sort" ; 200; "iv@<iv" )
        ("bool compar"; 200; "bv|1!bv" )
        ("iota\t" ; 200; "iv?/:_ fv" ))

time: {t:0; s:"\\t ",y; do[x; t+:.:s]; t}
wout: {`0:x,"\t",($y),"\n"}
run : {wout[x[0]; t:time[x[1];x[2]]]; t}

`0: "Running Tests...\n\n"

run'tests
\\
