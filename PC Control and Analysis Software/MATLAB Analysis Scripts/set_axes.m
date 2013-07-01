function set_axes(time,intens)

figure(7)
for i=1:6
subplot(3,2,i)
axis([time(1),time(2),intens(1),intens(2)]);
end