class CreateClubs < ActiveRecord::Migration
  def change
    create_table :clubs do |t|
      t.string :Name
      t.string :Admin
      t.integer :NumberOfMembers
      t.string :category

      t.timestamps
    end
  end
end
