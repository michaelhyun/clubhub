# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ name: 'Chicago' }, { name: 'Copenhagen' }])
#   Mayor.create(name: 'Emanuel', city: cities.first)
Category.create({genre: 'Sports'})
Category.create({genre: 'Multicultural'})
Category.create({genre: 'Science'})
Category.create({genre: 'Engineering'})

Clubs.create({name: MichaelsClub}, {Admin: michael}, {NumberOfMembers: '3'}, {Category: Sports})
