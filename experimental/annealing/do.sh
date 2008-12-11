for i in $(seq 31 60); do
  python my_anneal.py $i || break
done
