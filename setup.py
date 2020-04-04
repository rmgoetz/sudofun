from setuptools import setup

setup(name='sudofun',
      version='0.1',
      description='A package for sudoku tools',
      long_description=open('README.txt').read(),
      install_requires=[],
      classifiers=[
            'Development Status :: 1 - Planning',
            'Intended Audience :: Education',
            'License :: OSI Approved :: MIT License',
            'Natural Language :: English',
            'Programming Language :: Python :: 3.7',
            'Topic :: Games/Entertainment :: Puzzle Games'    
      ],
      url='https://github.com/rmgoetz/sudofun',
      author='Ryan Goetz',
      license='MIT',
      packages=['sudofun'],
      zip_safe=False)