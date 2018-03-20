import os
for _, _, files in os.walk('./_posts'):
  for file in files:
    data = ''
    with open('./_posts/'+file, 'r') as f:
      for line in f:
        data += line.replace('\n', '  \n')
      f.close()
    with open('./_posts/'+file, 'w') as f:
      f.write(data)
      f.close()
