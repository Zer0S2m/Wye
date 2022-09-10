import os

from setuptools import (
    setup, Extension
)


def get_long_description():
    """
    Return the README.
    """
    with open("README.md", encoding="utf8") as f:
        return f.read()


def get_packages(package):
    """
    Return root package and all sub-packages.
    """
    return [
        dirpath
        for dirpath, dirnames, filenames in os.walk(package)
        if os.path.exists(os.path.join(dirpath, "__init__.py"))
    ]


def install_wye():
    setup(
        name="wye",
        version="1.0.0",
        author="Aleksandr Silkin",
        description="Lightweight ASGI Library",
        package_dir={"wye": "wye"},
        packages=get_packages("wye"),
        include_package_data=True,
        python_requires=">=3.7",
        long_description=get_long_description(),
        long_description_content_type="text/markdown",
        project_urls={
            "Documentation": "https://github.com/Zer0S2m/Wye",
            "Source": "https://github.com/Zer0S2m/Wye",
        },
        ext_modules=[
            Extension(
                "wye_serializers",
                ["./wye/lib/serializers/src/core/core.c"],
                include_dirs=['./wye/lib/serializers/src/'],
                optional=False,
                language="c"
            )
        ]
    )


if __name__ == "__main__":
    install_wye()
