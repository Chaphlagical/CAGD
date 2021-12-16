% run the following command to enable the current Matlab session as a server
% enableservice('AutomationServer', true)

% to start the c++/OpenGL application, run the next line
% !start glvu.exe

nv = size(x, 1);
n = numel(P2PVtxIds);

d = 1000;
p = 1;

%% construct matrix for linear system
psrc = x(P2PVtxIds,:);

P = zeros(n,4);
Phi = zeros(n);
b = p2pDsts;

for i = 1:n
   for j = 1:n
        Phi(i, j) = norm(psrc(i,:)-psrc(j,:))^3;
   end
end

% trivariate quadratic polynomials:
% 1+x1+x2+x3

for i = 1:n
    P(i,1) = 1;
    P(i,2) = psrc(i,1);
    P(i,3) = psrc(i,2);
    P(i,4) = psrc(i,3);
end

A = [Phi P;P' zeros(4)];
B = [b; zeros(4, 3)];

%% solve for warp coefficients
coeff = linsolve(A, B);

result = zeros(size(x));

if n >1 && det(A) ~= 0
    for i = 1:n
        result = result + repmat(coeff(i,:),length(x),1).*(vecnorm((repmat(psrc(i,:),length(x),1)-x)')'.^3);
    end

    result = result + repmat(coeff(n+1,:),length(x),1)*1;
    result = result + repmat(coeff(n+2,:),length(x),1).*x(:,1);
    result = result + repmat(coeff(n+3,:),length(x),1).*x(:,2);
    result = result + repmat(coeff(n+4,:),length(x),1).*x(:,3);
else
    result = x + repmat(p2pDsts - psrc,length(x),1);
end

%% apply the map to x, output to var y, which will be read by the c++ application
y = result;
