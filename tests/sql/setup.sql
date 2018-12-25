GRANT ALL PRIVILEGES ON DATABASE sancho_test_db TO sancho_test;

CREATE TABLE country (
	   id integer PRIMARY KEY,
	   country_code varchar(2) NOT NULL UNIQUE,
	   country_name varchar(100) NOT NULL
);

CREATE SEQUENCE country_seq START 1;

CREATE TABLE city (
	   id integer PRIMARY KEY,
	   country_code varchar(2) NOT NULL REFERENCES country(country_code),
	   city_name varchar(100) NOT NULL
);

CREATE TABLE city_audit (
       city_name varchar(100) NOT NULL,
       entry_date timestamp NOT NULL
);

DROP SEQUENCE IF EXISTS country_seq;
DROP SEQUENCE IF EXISTS city_seq;

CREATE SEQUENCE country_seq START 1;
CREATE SEQUENCE city_seq START 1;

INSERT INTO country VALUES (nextval('country_seq'), 'AF', 'Afghanistan');
INSERT INTO country VALUES (nextval('country_seq'), 'AL', 'Albania');
INSERT INTO country VALUES (nextval('country_seq'), 'DZ', 'Algeria');
INSERT INTO country VALUES (nextval('country_seq'), 'DS', 'American Samoa');
INSERT INTO country VALUES (nextval('country_seq'), 'AD', 'Andorra');
INSERT INTO country VALUES (nextval('country_seq'), 'AO', 'Angola');
INSERT INTO country VALUES (nextval('country_seq'), 'AI', 'Anguilla');
INSERT INTO country VALUES (nextval('country_seq'), 'AQ', 'Antarctica');
INSERT INTO country VALUES (nextval('country_seq'), 'AG', 'Antigua and Barbuda');
INSERT INTO country VALUES (nextval('country_seq'), 'AR', 'Argentina');
INSERT INTO country VALUES (nextval('country_seq'), 'AM', 'Armenia');
INSERT INTO country VALUES (nextval('country_seq'), 'AW', 'Aruba');
INSERT INTO country VALUES (nextval('country_seq'), 'AU', 'Australia');
INSERT INTO country VALUES (nextval('country_seq'), 'AT', 'Austria');
INSERT INTO country VALUES (nextval('country_seq'), 'AZ', 'Azerbaijan');
INSERT INTO country VALUES (nextval('country_seq'), 'BS', 'Bahamas');
INSERT INTO country VALUES (nextval('country_seq'), 'BH', 'Bahrain');
INSERT INTO country VALUES (nextval('country_seq'), 'BD', 'Bangladesh');
INSERT INTO country VALUES (nextval('country_seq'), 'BB', 'Barbados');
INSERT INTO country VALUES (nextval('country_seq'), 'BY', 'Belarus');
INSERT INTO country VALUES (nextval('country_seq'), 'BE', 'Belgium');
INSERT INTO country VALUES (nextval('country_seq'), 'BZ', 'Belize');
INSERT INTO country VALUES (nextval('country_seq'), 'BJ', 'Benin');
INSERT INTO country VALUES (nextval('country_seq'), 'BM', 'Bermuda');
INSERT INTO country VALUES (nextval('country_seq'), 'BT', 'Bhutan');
INSERT INTO country VALUES (nextval('country_seq'), 'BO', 'Bolivia');
INSERT INTO country VALUES (nextval('country_seq'), 'BA', 'Bosnia and Herzegovina');
INSERT INTO country VALUES (nextval('country_seq'), 'BW', 'Botswana');
INSERT INTO country VALUES (nextval('country_seq'), 'BV', 'Bouvet Island');
INSERT INTO country VALUES (nextval('country_seq'), 'BR', 'Brazil');
INSERT INTO country VALUES (nextval('country_seq'), 'IO', 'British Indian Ocean Territory');
INSERT INTO country VALUES (nextval('country_seq'), 'BN', 'Brunei Darussalam');
INSERT INTO country VALUES (nextval('country_seq'), 'BG', 'Bulgaria');
INSERT INTO country VALUES (nextval('country_seq'), 'BF', 'Burkina Faso');
INSERT INTO country VALUES (nextval('country_seq'), 'BI', 'Burundi');
INSERT INTO country VALUES (nextval('country_seq'), 'KH', 'Cambodia');
INSERT INTO country VALUES (nextval('country_seq'), 'CM', 'Cameroon');
INSERT INTO country VALUES (nextval('country_seq'), 'CA', 'Canada');
INSERT INTO country VALUES (nextval('country_seq'), 'CV', 'Cape Verde');
INSERT INTO country VALUES (nextval('country_seq'), 'KY', 'Cayman Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'CF', 'Central African Republic');
INSERT INTO country VALUES (nextval('country_seq'), 'TD', 'Chad');
INSERT INTO country VALUES (nextval('country_seq'), 'CL', 'Chile');
INSERT INTO country VALUES (nextval('country_seq'), 'CN', 'China');
INSERT INTO country VALUES (nextval('country_seq'), 'CX', 'Christmas Island');
INSERT INTO country VALUES (nextval('country_seq'), 'CC', 'Cocos (Keeling) Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'CO', 'Colombia');
INSERT INTO country VALUES (nextval('country_seq'), 'KM', 'Comoros');
INSERT INTO country VALUES (nextval('country_seq'), 'CG', 'Congo');
INSERT INTO country VALUES (nextval('country_seq'), 'CK', 'Cook Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'CR', 'Costa Rica');
INSERT INTO country VALUES (nextval('country_seq'), 'HR', 'Croatia (Hrvatska)');
INSERT INTO country VALUES (nextval('country_seq'), 'CU', 'Cuba');
INSERT INTO country VALUES (nextval('country_seq'), 'CY', 'Cyprus');
INSERT INTO country VALUES (nextval('country_seq'), 'CZ', 'Czech Republic');
INSERT INTO country VALUES (nextval('country_seq'), 'DK', 'Denmark');
INSERT INTO country VALUES (nextval('country_seq'), 'DJ', 'Djibouti');
INSERT INTO country VALUES (nextval('country_seq'), 'DM', 'Dominica');
INSERT INTO country VALUES (nextval('country_seq'), 'DO', 'Dominican Republic');
INSERT INTO country VALUES (nextval('country_seq'), 'TP', 'East Timor');
INSERT INTO country VALUES (nextval('country_seq'), 'EC', 'Ecuador');
INSERT INTO country VALUES (nextval('country_seq'), 'EG', 'Egypt');
INSERT INTO country VALUES (nextval('country_seq'), 'SV', 'El Salvador');
INSERT INTO country VALUES (nextval('country_seq'), 'GQ', 'Equatorial Guinea');
INSERT INTO country VALUES (nextval('country_seq'), 'ER', 'Eritrea');
INSERT INTO country VALUES (nextval('country_seq'), 'EE', 'Estonia');
INSERT INTO country VALUES (nextval('country_seq'), 'ET', 'Ethiopia');
INSERT INTO country VALUES (nextval('country_seq'), 'FK', 'Falkland Islands (Malvinas)');
INSERT INTO country VALUES (nextval('country_seq'), 'FO', 'Faroe Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'FJ', 'Fiji');
INSERT INTO country VALUES (nextval('country_seq'), 'FI', 'Finland');
INSERT INTO country VALUES (nextval('country_seq'), 'FR', 'France');
INSERT INTO country VALUES (nextval('country_seq'), 'FX', 'France, Metropolitan');
INSERT INTO country VALUES (nextval('country_seq'), 'GF', 'French Guiana');
INSERT INTO country VALUES (nextval('country_seq'), 'PF', 'French Polynesia');
INSERT INTO country VALUES (nextval('country_seq'), 'TF', 'French Southern Territories');
INSERT INTO country VALUES (nextval('country_seq'), 'GA', 'Gabon');
INSERT INTO country VALUES (nextval('country_seq'), 'GM', 'Gambia');
INSERT INTO country VALUES (nextval('country_seq'), 'GE', 'Georgia');
INSERT INTO country VALUES (nextval('country_seq'), 'DE', 'Germany');
INSERT INTO country VALUES (nextval('country_seq'), 'GH', 'Ghana');
INSERT INTO country VALUES (nextval('country_seq'), 'GI', 'Gibraltar');
INSERT INTO country VALUES (nextval('country_seq'), 'GK', 'Guernsey');
INSERT INTO country VALUES (nextval('country_seq'), 'GR', 'Greece');
INSERT INTO country VALUES (nextval('country_seq'), 'GL', 'Greenland');
INSERT INTO country VALUES (nextval('country_seq'), 'GD', 'Grenada');
INSERT INTO country VALUES (nextval('country_seq'), 'GP', 'Guadeloupe');
INSERT INTO country VALUES (nextval('country_seq'), 'GU', 'Guam');
INSERT INTO country VALUES (nextval('country_seq'), 'GT', 'Guatemala');
INSERT INTO country VALUES (nextval('country_seq'), 'GN', 'Guinea');
INSERT INTO country VALUES (nextval('country_seq'), 'GW', 'Guinea-Bissau');
INSERT INTO country VALUES (nextval('country_seq'), 'GY', 'Guyana');
INSERT INTO country VALUES (nextval('country_seq'), 'HT', 'Haiti');
INSERT INTO country VALUES (nextval('country_seq'), 'HM', 'Heard and Mc Donald Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'HN', 'Honduras');
INSERT INTO country VALUES (nextval('country_seq'), 'HK', 'Hong Kong');
INSERT INTO country VALUES (nextval('country_seq'), 'HU', 'Hungary');
INSERT INTO country VALUES (nextval('country_seq'), 'IS', 'Iceland');
INSERT INTO country VALUES (nextval('country_seq'), 'IN', 'India');
INSERT INTO country VALUES (nextval('country_seq'), 'IM', 'Isle of Man');
INSERT INTO country VALUES (nextval('country_seq'), 'ID', 'Indonesia');
INSERT INTO country VALUES (nextval('country_seq'), 'IR', 'Iran (Islamic Republic of)');
INSERT INTO country VALUES (nextval('country_seq'), 'IQ', 'Iraq');
INSERT INTO country VALUES (nextval('country_seq'), 'IE', 'Ireland');
INSERT INTO country VALUES (nextval('country_seq'), 'IL', 'Israel');
INSERT INTO country VALUES (nextval('country_seq'), 'IT', 'Italy');
INSERT INTO country VALUES (nextval('country_seq'), 'CI', 'Ivory Coast');
INSERT INTO country VALUES (nextval('country_seq'), 'JE', 'Jersey');
INSERT INTO country VALUES (nextval('country_seq'), 'JM', 'Jamaica');
INSERT INTO country VALUES (nextval('country_seq'), 'JP', 'Japan');
INSERT INTO country VALUES (nextval('country_seq'), 'JO', 'Jordan');
INSERT INTO country VALUES (nextval('country_seq'), 'KZ', 'Kazakhstan');
INSERT INTO country VALUES (nextval('country_seq'), 'KE', 'Kenya');
INSERT INTO country VALUES (nextval('country_seq'), 'KI', 'Kiribati');
INSERT INTO country VALUES (nextval('country_seq'), 'KP', 'Korea, Democratic People''s Republic of');
INSERT INTO country VALUES (nextval('country_seq'), 'KR', 'Korea, Republic of');
INSERT INTO country VALUES (nextval('country_seq'), 'XK', 'Kosovo');
INSERT INTO country VALUES (nextval('country_seq'), 'KW', 'Kuwait');
INSERT INTO country VALUES (nextval('country_seq'), 'KG', 'Kyrgyzstan');
INSERT INTO country VALUES (nextval('country_seq'), 'LA', 'Lao People''s Democratic Republic');
INSERT INTO country VALUES (nextval('country_seq'), 'LV', 'Latvia');
INSERT INTO country VALUES (nextval('country_seq'), 'LB', 'Lebanon');
INSERT INTO country VALUES (nextval('country_seq'), 'LS', 'Lesotho');
INSERT INTO country VALUES (nextval('country_seq'), 'LR', 'Liberia');
INSERT INTO country VALUES (nextval('country_seq'), 'LY', 'Libyan Arab Jamahiriya');
INSERT INTO country VALUES (nextval('country_seq'), 'LI', 'Liechtenstein');
INSERT INTO country VALUES (nextval('country_seq'), 'LT', 'Lithuania');
INSERT INTO country VALUES (nextval('country_seq'), 'LU', 'Luxembourg');
INSERT INTO country VALUES (nextval('country_seq'), 'MO', 'Macau');
INSERT INTO country VALUES (nextval('country_seq'), 'MK', 'Macedonia');
INSERT INTO country VALUES (nextval('country_seq'), 'MG', 'Madagascar');
INSERT INTO country VALUES (nextval('country_seq'), 'MW', 'Malawi');
INSERT INTO country VALUES (nextval('country_seq'), 'MY', 'Malaysia');
INSERT INTO country VALUES (nextval('country_seq'), 'MV', 'Maldives');
INSERT INTO country VALUES (nextval('country_seq'), 'ML', 'Mali');
INSERT INTO country VALUES (nextval('country_seq'), 'MT', 'Malta');
INSERT INTO country VALUES (nextval('country_seq'), 'MH', 'Marshall Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'MQ', 'Martinique');
INSERT INTO country VALUES (nextval('country_seq'), 'MR', 'Mauritania');
INSERT INTO country VALUES (nextval('country_seq'), 'MU', 'Mauritius');
INSERT INTO country VALUES (nextval('country_seq'), 'TY', 'Mayotte');
INSERT INTO country VALUES (nextval('country_seq'), 'MX', 'Mexico');
INSERT INTO country VALUES (nextval('country_seq'), 'FM', 'Micronesia, Federated States of');
INSERT INTO country VALUES (nextval('country_seq'), 'MD', 'Moldova, Republic of');
INSERT INTO country VALUES (nextval('country_seq'), 'MC', 'Monaco');
INSERT INTO country VALUES (nextval('country_seq'), 'MN', 'Mongolia');
INSERT INTO country VALUES (nextval('country_seq'), 'ME', 'Montenegro');
INSERT INTO country VALUES (nextval('country_seq'), 'MS', 'Montserrat');
INSERT INTO country VALUES (nextval('country_seq'), 'MA', 'Morocco');
INSERT INTO country VALUES (nextval('country_seq'), 'MZ', 'Mozambique');
INSERT INTO country VALUES (nextval('country_seq'), 'MM', 'Myanmar');
INSERT INTO country VALUES (nextval('country_seq'), 'NA', 'Namibia');
INSERT INTO country VALUES (nextval('country_seq'), 'NR', 'Nauru');
INSERT INTO country VALUES (nextval('country_seq'), 'NP', 'Nepal');
INSERT INTO country VALUES (nextval('country_seq'), 'NL', 'Netherlands');
INSERT INTO country VALUES (nextval('country_seq'), 'AN', 'Netherlands Antilles');
INSERT INTO country VALUES (nextval('country_seq'), 'NC', 'New Caledonia');
INSERT INTO country VALUES (nextval('country_seq'), 'NZ', 'New Zealand');
INSERT INTO country VALUES (nextval('country_seq'), 'NI', 'Nicaragua');
INSERT INTO country VALUES (nextval('country_seq'), 'NE', 'Niger');
INSERT INTO country VALUES (nextval('country_seq'), 'NG', 'Nigeria');
INSERT INTO country VALUES (nextval('country_seq'), 'NU', 'Niue');
INSERT INTO country VALUES (nextval('country_seq'), 'NF', 'Norfolk Island');
INSERT INTO country VALUES (nextval('country_seq'), 'MP', 'Northern Mariana Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'NO', 'Norway');
INSERT INTO country VALUES (nextval('country_seq'), 'OM', 'Oman');
INSERT INTO country VALUES (nextval('country_seq'), 'PK', 'Pakistan');
INSERT INTO country VALUES (nextval('country_seq'), 'PW', 'Palau');
INSERT INTO country VALUES (nextval('country_seq'), 'PS', 'Palestine');
INSERT INTO country VALUES (nextval('country_seq'), 'PA', 'Panama');
INSERT INTO country VALUES (nextval('country_seq'), 'PG', 'Papua New Guinea');
INSERT INTO country VALUES (nextval('country_seq'), 'PY', 'Paraguay');
INSERT INTO country VALUES (nextval('country_seq'), 'PE', 'Peru');
INSERT INTO country VALUES (nextval('country_seq'), 'PH', 'Philippines');
INSERT INTO country VALUES (nextval('country_seq'), 'PN', 'Pitcairn');
INSERT INTO country VALUES (nextval('country_seq'), 'PL', 'Poland');
INSERT INTO country VALUES (nextval('country_seq'), 'PT', 'Portugal');
INSERT INTO country VALUES (nextval('country_seq'), 'PR', 'Puerto Rico');
INSERT INTO country VALUES (nextval('country_seq'), 'QA', 'Qatar');
INSERT INTO country VALUES (nextval('country_seq'), 'RE', 'Reunion');
INSERT INTO country VALUES (nextval('country_seq'), 'RO', 'Romania');
INSERT INTO country VALUES (nextval('country_seq'), 'RU', 'Russian Federation');
INSERT INTO country VALUES (nextval('country_seq'), 'RW', 'Rwanda');
INSERT INTO country VALUES (nextval('country_seq'), 'KN', 'Saint Kitts and Nevis');
INSERT INTO country VALUES (nextval('country_seq'), 'LC', 'Saint Lucia');
INSERT INTO country VALUES (nextval('country_seq'), 'VC', 'Saint Vincent and the Grenadines');
INSERT INTO country VALUES (nextval('country_seq'), 'WS', 'Samoa');
INSERT INTO country VALUES (nextval('country_seq'), 'SM', 'San Marino');
INSERT INTO country VALUES (nextval('country_seq'), 'ST', 'Sao Tome and Principe');
INSERT INTO country VALUES (nextval('country_seq'), 'SA', 'Saudi Arabia');
INSERT INTO country VALUES (nextval('country_seq'), 'SN', 'Senegal');
INSERT INTO country VALUES (nextval('country_seq'), 'RS', 'Serbia');
INSERT INTO country VALUES (nextval('country_seq'), 'SC', 'Seychelles');
INSERT INTO country VALUES (nextval('country_seq'), 'SL', 'Sierra Leone');
INSERT INTO country VALUES (nextval('country_seq'), 'SG', 'Singapore');
INSERT INTO country VALUES (nextval('country_seq'), 'SK', 'Slovakia');
INSERT INTO country VALUES (nextval('country_seq'), 'SI', 'Slovenia');
INSERT INTO country VALUES (nextval('country_seq'), 'SB', 'Solomon Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'SO', 'Somalia');
INSERT INTO country VALUES (nextval('country_seq'), 'ZA', 'South Africa');
INSERT INTO country VALUES (nextval('country_seq'), 'GS', 'South Georgia South Sandwich Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'ES', 'Spain');
INSERT INTO country VALUES (nextval('country_seq'), 'LK', 'Sri Lanka');
INSERT INTO country VALUES (nextval('country_seq'), 'SH', 'St. Helena');
INSERT INTO country VALUES (nextval('country_seq'), 'PM', 'St. Pierre and Miquelon');
INSERT INTO country VALUES (nextval('country_seq'), 'SD', 'Sudan');
INSERT INTO country VALUES (nextval('country_seq'), 'SR', 'Suriname');
INSERT INTO country VALUES (nextval('country_seq'), 'SJ', 'Svalbard and Jan Mayen Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'SZ', 'Swaziland');
INSERT INTO country VALUES (nextval('country_seq'), 'SE', 'Sweden');
INSERT INTO country VALUES (nextval('country_seq'), 'CH', 'Switzerland');
INSERT INTO country VALUES (nextval('country_seq'), 'SY', 'Syrian Arab Republic');
INSERT INTO country VALUES (nextval('country_seq'), 'TW', 'Taiwan');
INSERT INTO country VALUES (nextval('country_seq'), 'TJ', 'Tajikistan');
INSERT INTO country VALUES (nextval('country_seq'), 'TZ', 'Tanzania, United Republic of');
INSERT INTO country VALUES (nextval('country_seq'), 'TH', 'Thailand');
INSERT INTO country VALUES (nextval('country_seq'), 'TG', 'Togo');
INSERT INTO country VALUES (nextval('country_seq'), 'TK', 'Tokelau');
INSERT INTO country VALUES (nextval('country_seq'), 'TO', 'Tonga');
INSERT INTO country VALUES (nextval('country_seq'), 'TT', 'Trinidad and Tobago');
INSERT INTO country VALUES (nextval('country_seq'), 'TN', 'Tunisia');
INSERT INTO country VALUES (nextval('country_seq'), 'TR', 'Turkey');
INSERT INTO country VALUES (nextval('country_seq'), 'TM', 'Turkmenistan');
INSERT INTO country VALUES (nextval('country_seq'), 'TC', 'Turks and Caicos Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'TV', 'Tuvalu');
INSERT INTO country VALUES (nextval('country_seq'), 'UG', 'Uganda');
INSERT INTO country VALUES (nextval('country_seq'), 'UA', 'Ukraine');
INSERT INTO country VALUES (nextval('country_seq'), 'AE', 'United Arab Emirates');
INSERT INTO country VALUES (nextval('country_seq'), 'GB', 'United Kingdom');
INSERT INTO country VALUES (nextval('country_seq'), 'US', 'United States');
INSERT INTO country VALUES (nextval('country_seq'), 'UM', 'United States minor outlying islands');
INSERT INTO country VALUES (nextval('country_seq'), 'UY', 'Uruguay');
INSERT INTO country VALUES (nextval('country_seq'), 'UZ', 'Uzbekistan');
INSERT INTO country VALUES (nextval('country_seq'), 'VU', 'Vanuatu');
INSERT INTO country VALUES (nextval('country_seq'), 'VA', 'Vatican City State');
INSERT INTO country VALUES (nextval('country_seq'), 'VE', 'Venezuela');
INSERT INTO country VALUES (nextval('country_seq'), 'VN', 'Vietnam');
INSERT INTO country VALUES (nextval('country_seq'), 'VG', 'Virgin Islands (British)');
INSERT INTO country VALUES (nextval('country_seq'), 'VI', 'Virgin Islands (U.S.)');
INSERT INTO country VALUES (nextval('country_seq'), 'WF', 'Wallis and Futuna Islands');
INSERT INTO country VALUES (nextval('country_seq'), 'EH', 'Western Sahara');
INSERT INTO country VALUES (nextval('country_seq'), 'YE', 'Yemen');
INSERT INTO country VALUES (nextval('country_seq'), 'ZR', 'Zaire');
INSERT INTO country VALUES (nextval('country_seq'), 'ZM', 'Zambia');
INSERT INTO country VALUES (nextval('country_seq'), 'ZW', 'Zimbabwe');

INSERT INTO city VALUES (nextval('city_seq'), 'GB', 'London');
INSERT INTO city VALUES (nextval('city_seq'), 'GB', 'Liverpool');
INSERT INTO city VALUES (nextval('city_seq'), 'GB', 'Manchester');
INSERT INTO city VALUES (nextval('city_seq'), 'PL', 'Warsaw');
INSERT INTO city VALUES (nextval('city_seq'), 'PL', 'Krakow');
INSERT INTO city VALUES (nextval('city_seq'), 'US', 'New York');
INSERT INTO city VALUES (nextval('city_seq'), 'US', 'Boston');
INSERT INTO city VALUES (nextval('city_seq'), 'US', 'Chicago');

CREATE VIEW cities_and_countries AS
       SELECT
            city.id as city_id,
            city.city_name,
            country.country_name,
            city.country_code
       FROM
            city
       JOIN
            country on country.country_code = city.country_code
       ORDER BY country.country_name;


CREATE OR REPLACE FUNCTION insert_city_audit() RETURNS TRIGGER AS $$
   BEGIN
      INSERT INTO city_audit(city_name, entry_date) VALUES (new.city_name, timezone('utc', now()));
      RETURN NEW;
   END;
$$ LANGUAGE plpgsql;


CREATE TRIGGER city_trigger AFTER INSERT ON city
FOR EACH ROW EXECUTE PROCEDURE insert_city_audit();
