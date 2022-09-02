from setuptools import (
    setup, Extension
)


with open("README.md", "r", encoding="utf-8") as f:
    readme = f.read()


def install_wye():
    setup(
        name="wye",
        version="1.0.0",
        author="Aleksandr Silkin",
        description="Lightweight ASGI Library",
        package_dir={"wye": "wye"},
        packages=["wye"],
        package_data={"": ["LICENSE"]},
        include_package_data=True,
        python_requires=">=3.9",
        long_description=readme,
        long_description_content_type="text/markdown",
        project_urls={
            "Documentation": "https://github.com/Zer0S2m/Wye",
            "Source": "https://github.com/Zer0S2m/Wye",
        },
        ext_modules=[
            Extension(
                "wye_serializers",
                ["./wye/serializers/src/core/core.c"],
                include_dirs=['./wye/serializers/src/'],
                optional=False,
                language="c"
            )
        ]
    )


if __name__ == "__main__":
    install_wye()
