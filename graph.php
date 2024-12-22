<?php

$infile=fopen($argv[1], "r");

$cols=rtrim(fgets($infile));

$imgdata=array();

while (!feof($infile))
{
  $line=array();
  for ($i=0; $i<$cols; ++$i)
    $line[]=rtrim(fgets($infile));
  $imgdata[]=$line;
}

fclose($infile);

$rows=sizeof($imgdata);

$min=$imagedata[0][0];
$max=$min;

print("Data size: ".$cols." x ".$rows."\n");

for ($y=0; $y<$rows; ++$y)
{
  print("Check row: ".$y."...\n");
  for ($x=0; $x<$cols; ++$x)
  {
    $c=$imagedata[$y][$x];
    if ($c>$max) $max=$c;
    if ($c<$min) $min=$c;
  }
}

$range=$max-$min;
$ratio=255/$range;

print("Min: ".$min." Max: ".$max." Range: ".$range." Ratio: ".$ratio."\n");

$sx=1;
$sy=1;

$gid=imagecreatetruecolor($cols*$sx, $rows*$sy);
$colors=array();
for ($i=0; $i<256; ++$i) $colors[]=imagecolorallocate($gid, $i, $i, $i);
imagefilledrectangle($gid, 0, 0, $cols, $rows, $colors[0]);

for ($y=0; $y<$rows; ++$y)
{
  for ($x=0; $x<$cols; ++$x)
  {
    $c=($imgdata[$y][$x]-$min)/$range;
    $px=$x*$sx;
    $py=$y*$sy;
    imagefilledrectangle($gid, $px, $py, $px+$sx, $py+$sy, $colors[$c]);
  }
}

imagepng($gid, $argv[1].".png");

?>
