import argparse

from goose.enums.mode import Mode
from goose.enums.policy_type import PolicyType
from goose.enums.state_representation import StateRepresentation
from goose.util.parseable_enum import ParseableEnum


def namespace_to_serialisable(opts: argparse.Namespace) -> argparse.Namespace:
    for key, val in vars(opts).items():
        if issubclass(type(val), ParseableEnum):
            val = val.value
        opts.__dict__[key] = val
    return opts


def namespace_from_serialisable(opts: argparse.Namespace) -> argparse.Namespace:
    for key, cls in [
        ("mode", Mode),
        ("policy_type", PolicyType),
        ("state_representation", StateRepresentation),
    ]:
        val = opts.__dict__[key]
        if isinstance(val, str):
            try:
                opts.__dict__[key] = cls.parse(val)
            except ValueError as e:
                raise ValueError(f"Invalid value for {key=}") from e
    return opts
