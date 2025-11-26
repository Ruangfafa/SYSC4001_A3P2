#!/bin/bash

cd "$(dirname "$0")"
RES_DIR="../resources"

mkdir -p "$RES_DIR/exams"

cat > "$RES_DIR/rubric.txt" <<EOF
1,A
2,B
3,C
4,D
5,E
EOF

for i in $(seq -w 1 19); do
    printf "%04d\n" $(( RANDOM % 9998 + 1 )) > "$RES_DIR/exams/exam_${i}.txt"
done

echo "9999" > "$RES_DIR/exams/exam_20.txt"
