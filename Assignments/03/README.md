Initial Commit: Just had everything running as is. \
Second Commit: Implemented a simple async task that tackles each request asyncronously. \
    Assigned to the same future value so that it actually performs asyncronously vs. waiting for the future \
    within the scope of the while loop.
Third Commit: Had a circular buffer implementation, but the image files were corrupt. I figured most of the \
    time is being spent fetching the data, and then sending that data. So I had the producer adding responses \
    to the buffer, and then the consumer sending those responses to the client. \
Fourth Commit: Now I actually have the previous implementation working. 

| Commit | Time | Times Faster |
| ------ | ---- | ----------  |
| 1 | 1.92 min | 1 |
| 2 | 1.1 min | 1.74 |
| 3 | 32.64s | 3.53 |
| 4 | 26.89s | 4.28 |
