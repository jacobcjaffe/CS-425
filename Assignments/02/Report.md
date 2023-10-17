Commits
1: created a bash file to quickly automate testing
2: utilized the getNext function to retrieve the next number. 
3: created two iterators. Have half of the threads getNext from the front, and
    the other half retrieving from the back of the numbers.
4: Tried creating a queue, but timing is worse. I think it may have to do with copying the
    data repeatedly
5: Simply used the getNext using a vector

Inital Timing:
Real: .659s
User: 4.141s
Sys: .005s

Commit 2:
Real: .633s
User: 4.038s
Sys: .010s

Commit 3:
Real: .486s
User: 4.672s
Sys .002s

Commit 4:
Real: .534s
User: 4.268s
Sys: .007s

Commit 5: 
Real: .432s
User: 3.893s
Sys: .004s

Final Times:
Real: 1m47.949s
User: 18m1.492s
Sys: 1.015s
