clear all;
func = @(x)(2.456*x+361348.15);

x = linspace(341654.828, 341661.215, 20);
x = x + 1.0*rand(size(x));
y = func(x) + 1.0*rand(size(x));
figure(1);
plot(x, y,'rx');

a = polyfit(x,y,1);
hold on;
wp = @(xx)(a(1)*xx+a(2));
fplot(wp, minmax(x)); 
wp1 = wp(x(1)-0.5);
wp2 = wp(x(end)+3);
plot(x(1)-0.5, wp1,'bx','MarkerSize',15);
plot(x(end)+3,wp2,'bx','MarkerSize',10);
hold off;
