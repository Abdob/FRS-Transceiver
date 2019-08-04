s1 = FRSsingle{:,1}';
s1 = s1/mean(abs(s1));
s2 = FRSsingle{:,2}';
s2 = s2/mean(abs(s2));
s = s1 + s2*j;
DataToGRC('singleFRS', s,'complex');



d1a = FRSdual{:,1}';
d1a = d1a/mean(abs(d1a));

d1b = FRSdual{:,2}';
d1b = d1b/mean(abs(d1b));

d1 = d1a + d1b*j;
DataToGRC('dualFRS1', d1,'complex');

d2a = FRSdual{:,3}';
d2a = d1a/mean(abs(d2a));

d2b = FRSdual{:,4}';
d2b = d1b/mean(abs(d2b));

d2 = d2a + d2b*j;
DataToGRC('dualFRS2', d2,'complex');
