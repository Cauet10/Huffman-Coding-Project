T = readtable("Huffman-Coding-Project/contagem e plotagem/dados.csv");
close all;
cores = lines(2);

G = groupsummary(T, 'n', 'mean', {'comp_lista', 'comp_arvore'});

figure;

subplot(1,1,1);
loglog(T.n, T.comp_lista,  '.', 'Color', 0.6 + 0.4*cores(1,:)); hold on;
loglog(T.n, T.comp_arvore, '.', 'Color', 0.6 + 0.4*cores(2,:));
h1 = loglog(G.n, G.mean_comp_lista,  '-o', 'LineWidth', 2, 'Color', cores(1,:));
h2 = loglog(G.n, G.mean_comp_arvore, '-s', 'LineWidth', 2, 'Color', cores(2,:));
grid on;
xlabel('Número de itens (n)');
ylabel('Comparações');
title('Escala log-log (pontos = sorteios)');
legend([h1 h2], {'Lista (média)', 'Árvore (média)'}, 'Location', 'northwest');