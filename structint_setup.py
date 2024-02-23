from setuptools import Extension, setup

def main():
  setup(
    name="structint",
    version="0.0.3",
    author="grasol",

    ext_modules=[
      Extension(
        name="structint",
        sources=["src/structint.c", "src/core.c"]
        )
      ]
    )

if __name__ == "__main__":
  main()