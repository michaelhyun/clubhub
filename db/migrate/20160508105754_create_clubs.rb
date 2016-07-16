class CreateClubs < ActiveRecord::Migration
  def change
    create_table :clubs do |t|
      t.string :name
      t.string :admin
      t.string :numberOfMembers
      t.text :description
      t.string :category
    end
  end
end
