for file in src/*;
do
	g++ -MM $file
done
