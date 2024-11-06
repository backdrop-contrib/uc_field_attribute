# Ubercart Field Attributes

This module links Ubercart's attribute system to Drupal core's fields API and
allows to automatically create product attributes from fields and options from
the multi-values.

In the product class settings there's a new tab called "field attributes" that
lists all existing fields of a product class. By selecting a field here it
becomes available as an attribute (generator) and multi-value options are copied
to ubercart product options when new products of this class are created and get
synchronised when their multi-values are changed.

For example, a date field can be used to automatically create options for each
date of a product node.

## Support

Bugs and Feature requests should be reported in the [Issue queue](https://github.com/backdrop-contrib/uc_field_attribute/issues).

## Maintainers

- [Herb v/d Dool](https://github.com/herbdool/)
- This module is seeking additional maintainers.

## Credits

Ported to Backdrop CMS by Herb v/d Dool.

Originally written for Drupal by

- [miiimooo](https://www.drupal.org/u/miiimooo)


## License

This project is GPL v3 software. See the LICENSE.txt file in this directory for
complete text.
