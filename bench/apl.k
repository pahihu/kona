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
N: 200
M: 20

tests: (("int add\t" ; N; "im+im+im+im+im")
        ("fp add\t" ; N; "fm+fm+fm+fm+fm")
        ("int mult" ; N; "im*im*im*im*im")
        ("fp mult\t" ; N; "fm*fm*fm*fm*fm")
        ("index\t" ; M*N; "(!#bv)@&bv" )
        ("char compr" ; M*N; "cv@&bv" )
        ("int compr" ; M*N; "iv@&bv" )
        ("int + red" ; M*N; "+/iv" )
        ("int | red" ; M*N; "|/iv" )
        ("bool scan" ; N%2; "(~=)\\bm" )
        ("mat rotate" ; N%2; "(!500)!\\:cm" )
        ("char trans" ; N%2; "+cm" )
        ("int trans" ; N%2; "+im" )
        ("vec of vecs"; M*N; ",:'cm" )
        ("partition" ; M*N; "aa:(&bv)_ cv" )
        ("shape each" ; M*N; "^:'aa" )
        ("vec compar" ; M*N; "&/cv=cv" )
        ("int sort" ; M*N; "iv@<iv" )
        ("bool compar"; M*N; "bv|1!bv" )
        ("iota\t" ; N; "iv?/:_ fv" ))

/time: {t:0; s:"\\t ",y; do[x; t+:.:s]; t}
time: {t:0; s:"\\t do[",($x),";",y,"]"; .:s}
wout: {`0:x,"\t",($y),"\n"}
run : {wout[x[0]; t:time[x[1];x[2]]]; t}

`0: "Running Tests...\n\n"

run'tests
\\
