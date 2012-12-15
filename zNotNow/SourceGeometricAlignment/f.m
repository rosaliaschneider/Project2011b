function val = f( H )
% returns the vector of costs for which the squared sum must be minimized

global x1 x2

n = length(x1);
val = zeros(n);

for i = 1:n

    val(i) = distance(transpose(x1(i, :)), H*transpose(x2(i, :))) + distance(transpose(x2(i, :)), inv(H)*transpose(x1(i, :)));

end

end

