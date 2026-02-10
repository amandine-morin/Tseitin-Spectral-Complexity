python3 - <<'PY'
import csv, statistics, math
from collections import defaultdict

path="/tmp/sweep_ws_n80_d4.csv"
times=defaultdict(list)
timeouts=defaultdict(int)
cnt=defaultdict(int)

with open(path) as f:
    r=csv.DictReader(f)
    for row in r:
        p=row["p"]
        t=int(row["runtime_ms"])
        cnt[p]+=1
        timeouts[p]+= int(t>=60000)
        times[p].append(min(t,60000))

def q(v, q):
    v=sorted(v)
    k=(len(v)-1)*q
    i=int(math.floor(k)); j=int(math.ceil(k))
    if i==j: return v[i]
    return int(round(v[i]*(j-k)+v[j]*(k-i)))

print("p,cnt,timeouts,timeout_rate,median_ms,q90_ms")
for p in sorted(times, key=float):
    v=times[p]
    print(f"{p},{cnt[p]},{timeouts[p]},{timeouts[p]/cnt[p]:.3f},{statistics.median(v):.0f},{q(v,0.90)}")
PY
