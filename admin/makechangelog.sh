#! /bin/sh

CVS2CL=admin/cvs2cl.pl

echo ""
echo "Generating ChangeLog from CVS logs..."
echo ""

$CVS2CL -f ChangeLog -W 10 -P -S --no-wrap

rm -f ChangeLog.bak

exit 0
